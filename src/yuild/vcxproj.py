# yuild/vcxproj.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the yuild project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yuild project nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from copy import copy
from cStringIO import StringIO
from os import getcwd
from os.path import dirname, exists, join, split, splitext
import traceback
from uuid import UUID, uuid4

from yutil import abspath, deduplist, fnmatch, indent, ntpath, ntpaths, relpath, strlist, strlistprefix, treepaths

from yuild.constant import C_CXX_INCLUDE_FILE_EXTENSIONS, \
                           C_CXX_SOURCE_FILE_EXTENSIONS
from yuild.platform_dict import PlatformDict
from yuild.vcproj import VCProj



__all__ = [
    "VCXProj",
    "VCXProjFilters",
    "VCXProjUser",
    "VCXSln",
]


# Constants
CONFIGURATION_TYPE = {
    "lib": "StaticLibrary",
    "dll": "DynamicLibrary",
    "exe": "Application",
    "gui": "Application",
}

SUBSYSTEM = {
    "dll": "WINDOWS",
    "exe": "CONSOLE",
    "gui": "WINDOWS"
}

INDENT_SPACES = ' ' * 2


# Templates
CL_COMPILE_EXCLUDED_ITEM = """\
<ClCompile Include="%(Include)s">
  <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">true</ExcludedFromBuild>
  <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">true</ExcludedFromBuild>
</ClCompile>
"""

CL_COMPILE_SOURCE_FILTERS_ITEM = """\
<ClCompile Include="%(Include)s">
  <Filter>%(Filter)s</Filter>
</ClCompile>
"""

CL_COMPILE_SOURCE_ITEM = """\
<ClCompile Include="%(Include)s">%(ObjectFileName)s
</ClCompile>
"""

CL_INCLUDE_FILTERS_ITEM = """\
<ClInclude Include="%(Include)s">
  <Filter>%(Filter)s</Filter>
</ClInclude>
"""

CL_INCLUDE_ITEM = """\
<ClInclude Include="%(Include)s" />
"""

CONFIGURATION_PROPERTY_GROUP = """\
<PropertyGroup %(Condition)s Label="Configuration">
  <ConfigurationType>%(ConfigurationType)s</ConfigurationType>
  <CharacterSet>Unicode</CharacterSet>
  <UseDebugLibraries>%(UseDebugLibraries)s</UseDebugLibraries>
</PropertyGroup>
"""

FILTER_ITEM = """\
<Filter Include="%(Include)s">
  <UniqueIdentifier>{%(UniqueIdentifier)s}</UniqueIdentifier>
</Filter>
"""

LIB_ITEM_DEFINITION = """\
<Lib>
  <AdditionalDependencies>%(AdditionalDependencies)s</AdditionalDependencies>
  <AdditionalLibraryDirectories>%(AdditionalLibraryDirectories)s</AdditionalLibraryDirectories>
  <AdditionalOptions>%(AdditionalOptions)s</AdditionalOptions>
  <GenerateDebugInformation>%(GenerateDebugInformation)s</GenerateDebugInformation>
</Lib>
"""

LINK_ITEM_DEFINITION = """\
<Link>
  <AdditionalDependencies>%(AdditionalDependencies)s</AdditionalDependencies>
  <AdditionalLibraryDirectories>%(AdditionalLibraryDirectories)s</AdditionalLibraryDirectories>
  <AdditionalOptions>%(AdditionalOptions)s</AdditionalOptions>
  <GenerateDebugInformation>%(GenerateDebugInformation)s</GenerateDebugInformation>
  <SubSystem>%(SubSystem)s</SubSystem>
</Link>
"""

OPTIONS_ITEM_DEFINITION_GROUP = """\
<ItemDefinitionGroup %(Condition)s>
  <ClCompile>
    <Optimization>Disabled</Optimization>
    <AdditionalIncludeDirectories>%(AdditionalIncludeDirectories)s</AdditionalIncludeDirectories>
    <PreprocessorDefinitions>%(PreprocessorDefinitions)s</PreprocessorDefinitions>
    <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
    <RuntimeLibrary>%(RuntimeLibrary)s</RuntimeLibrary>
    <RuntimeTypeInfo>false</RuntimeTypeInfo>
    <WarningLevel>Level4</WarningLevel>%(DebugInformationFormat)s
    <AdditionalOptions>%(AdditionalOptions)s</AdditionalOptions>
  </ClCompile>
%(LibLink)s
</ItemDefinitionGroup>
"""

PATHS_PROPERTY_GROUP = """\
<PropertyGroup>
  <IntDir %(Condition)s>%(IntDir)s</IntDir>
  <OutDir %(Condition)s>%(OutDir)s</OutDir>%(TargetName)s%(TargetExt)s
</PropertyGroup>
"""

PROJECT_CONFIGURATION_ITEM = """\
<ProjectConfiguration Include="%(Include)s">
  <Configuration>%(Configuration)s</Configuration>
  <Platform>%(Platform)s</Platform>
</ProjectConfiguration>
"""

PROJECT_REFERENCE_ITEM = """\
<ProjectReference Include="%(Include)s">
  <Project>{%(Project)s}</Project>
  <ReferenceOutputAssembly>false</ReferenceOutputAssembly>
</ProjectReference>
"""

USER_PROPS_IMPORT_GROUP = """\
<ImportGroup Label="PropertySheets" %(Condition)s>
  <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
</ImportGroup>
"""


class VCXProj(VCProj):
    def __get_header_items(self):
        return visit_source_file_tree(
                   self.get_include_file_tree(),
                   lambda header_file:\
                          CL_INCLUDE_ITEM % { "Include": ntpath(header_file.get_path()) }
               )

    def __get_source_items(self):
        def visit_source_file(source_file):
            Include = ntpath(source_file.get_path())

            if source_file.get_language() in ('c', "cpp"):
                source_file_ext = splitext(source_file.get_path())[1]

                if source_file_ext in C_CXX_INCLUDE_FILE_EXTENSIONS:
                    return CL_INCLUDE_ITEM % locals()
                elif source_file_ext in C_CXX_SOURCE_FILE_EXTENSIONS:
                    if source_file.get_platform() in ('*', "win32") and\
                       source_file not in self.get_exclude_files():
                        if dirname(source_file.get_path()) == self.get_root_source_dir_path():
                            ObjectFileName = ""
                        else:
                            ObjectFileName = "$(IntDir)"
                            ObjectFileName += ntpath(relpath(source_file.get_path(), self.get_root_source_dir_path()))
                            ObjectFileName = ObjectFileName[:ObjectFileName.rindex('.')] + ".obj"
                            ObjectFileName = "<ObjectFileName>%(ObjectFileName)s</ObjectFileName>" % locals()
                            ObjectFileName = '\n' + indent(INDENT_SPACES, ObjectFileName)

                        return CL_COMPILE_SOURCE_ITEM % locals()

            return CL_COMPILE_EXCLUDED_ITEM % locals()

        return visit_source_file_tree(
                   self.get_source_file_tree(),
                   visit_source_file
               )

    def __str__(self):
        AdditionalDependencies = self.get_AdditionalDependencies(';')
        AdditionalIncludeDirectories = self.get_AdditionalIncludeDirectories()
        AdditionalLibraryDirectories = self.get_AdditionalLibraryDirectories()
        PreprocessorDefinitions = self.get_PreprocessorDefinitions()
        try:
            ProjectGuid = \
                VCXProjReference(
                    self.get_project_dir_path(),
                    join(self.get_project_dir_path(), self.get_name() + ".vcxproj")
                ).get_Guid()
        except:
            print __name__ + ": generating GUID for project", self.get_name()
            ProjectGuid = str(uuid4())
        RootNamespace = self.get_RootNamespace()

        configuration_property_groups = []
        project_configuration_items = []
        paths_property_groups = []
        options_item_definition_groups = []
        user_props_import_groups = []

        for Platform in ("Win32",):
            for Configuration in ("Debug", "Release"):
                Condition = "'$(Configuration)|$(Platform)' == '%(Configuration)s|%(Platform)s'" % locals()
                Condition = "Condition=\"%(Condition)s\"" % locals()

                # Configuration property groups
                ConfigurationType = CONFIGURATION_TYPE[self.get_type()]
                UseDebugLibraries = Configuration == "Debug" and "true" or "false"
                configuration_property_group = CONFIGURATION_PROPERTY_GROUP % locals()
                configuration_property_group = indent(INDENT_SPACES, configuration_property_group)
                configuration_property_groups.append(configuration_property_group)

                # Project configuration item
                Include = Configuration + '|' + Platform
                project_configuration_item = PROJECT_CONFIGURATION_ITEM % locals()
                project_configuration_item = indent(INDENT_SPACES * 2, project_configuration_item)
                project_configuration_items.append(project_configuration_item)

                # Paths property group
                IntDir = self.get_IntermediateDirectory()
                OutDir = self.get_OutputDirectory()
                OutputFile = self.get_OutputFile()
                if OutputFile == self.get_name() or \
                   OutputFile == self.get_name() + '.' + self.get_type():
                    # OutputFile = TargetExt = TargetName = ""
                    OutputFile = TargetExt = TargetName = ""
                else:
                    TargetName, TargetExt = splitext(OutputFile)
                    if len(TargetExt) == 0 or TargetExt == '.' + self.get_type():
                        TargetExt = ""
                    else:
                        TargetExt = """<TargetExt %(Condition)s>%(TargetExt)s</TargetExt>""" % locals()
                        TargetExt = '\n' + indent(INDENT_SPACES, TargetExt)

                    if TargetName == self.get_name():
                        TargetName = ""
                    else:
                        TargetName = """<TargetName %(Condition)s>%(TargetName)s</TargetName>""" % locals()
                        TargetName = '\n' + indent(INDENT_SPACES, TargetName)

                paths_property_group = PATHS_PROPERTY_GROUP % locals()
                paths_property_group = indent(INDENT_SPACES, paths_property_group)
                paths_property_groups.append(paths_property_group)

                # Options item definition group
                if Configuration == "Debug":
                    RuntimeLibrary = "MultiThreadedDebugDLL"
                    # DebugInformationFormat = "EditAndContinue"
                    DebugInformationFormat = "OldStyle"
                    DebugInformationFormat = '\n' + \
                        indent(
                            INDENT_SPACES * 2,
                            "<DebugInformationFormat>%(DebugInformationFormat)s</DebugInformationFormat>" % locals()
                        )
                    GenerateDebugInformation = "true"
                else:
                    RuntimeLibrary = "MultiThreadedDLL"
                    DebugInformationFormat = ""
                    GenerateDebugInformation = "false"

                if ConfigurationType == "Application" or ConfigurationType == "DynamicLibrary":
                    SubSystem = SUBSYSTEM[self.get_type()]
                    AdditionalOptions = ' '.join(self.get_ldflags().get("win32", []))
                    LibLink = LINK_ITEM_DEFINITION % locals()
                else:
                    AdditionalOptions = ' '.join(self.get_ldflags().get("win32", []))
                    LibLink = LIB_ITEM_DEFINITION % locals()
                LibLink = indent(INDENT_SPACES, LibLink)

                AdditionalOptions = ' '.join(self.get_cxxflags().get("win32", []))

                options_item_definition_group = OPTIONS_ITEM_DEFINITION_GROUP % locals()
                options_item_definition_group = indent(INDENT_SPACES, options_item_definition_group)
                options_item_definition_groups.append(options_item_definition_group)

                # User properties import groups
                user_props_import_group = USER_PROPS_IMPORT_GROUP % locals()
                user_props_import_group = indent(INDENT_SPACES, user_props_import_group)
                user_props_import_groups.append(user_props_import_group)

        configuration_property_groups = '\n'.join(configuration_property_groups)
        project_configuration_items = '\n'.join(project_configuration_items)
        paths_property_groups = '\n'.join(paths_property_groups)
        options_item_definition_groups = '\n'.join(options_item_definition_groups)
        user_props_import_groups = '\n'.join(user_props_import_groups)

        header_items = self.__get_header_items()
        header_items = indent(INDENT_SPACES * 2, "".join(header_items))

        source_items = self.__get_source_items()
        source_items = indent(INDENT_SPACES * 2, "".join(source_items))

        project_reference_items = []
        for vcxproj_reference in \
            VCXProjReferences(
                self.get_project_dir_path(),
                self.get_project_references()
            ):
            Include = vcxproj_reference.get_Include()
            Project = vcxproj_reference.get_Guid()
            project_reference_items.append(PROJECT_REFERENCE_ITEM % locals())
        project_reference_items = \
            indent(
                INDENT_SPACES * 2,
                "".join(project_reference_items)
            )

        return """\
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
%(project_configuration_items)s
  </ItemGroup>

  <PropertyGroup Label="Globals">
    <ProjectGuid>{%(ProjectGuid)s}</ProjectGuid>
    <_ProjectFileVersion>10.0.30319.1</_ProjectFileVersion>
    <RootNamespace>%(RootNamespace)s</RootNamespace>
  </PropertyGroup>

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />

%(configuration_property_groups)s

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  
  <ImportGroup Label="ExtensionSettings" />

%(user_props_import_groups)s

%(paths_property_groups)s

%(options_item_definition_groups)s

  <ItemGroup Label="ClIncludeItems">
%(header_items)s
  </ItemGroup>

  <ItemGroup Label="ClCompileItems">
%(source_items)s
  </ItemGroup>

  <ItemGroup Label="ProjectReferenceItems">
%(project_reference_items)s
  </ItemGroup>

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
""" % locals()


class VCXProjFilters(object):
    def __init__(self, *args, **kwds):
        self.__vcxproj = VCXProj(*args, **kwds)

    def __get_FilterUniqueIdentifier(self, vcxproj_filters_file_path, FilterInclude):
        Filter = '<Filter Include="%(FilterInclude)s">' % locals()

        try:
            vcxproj_filters_file = open(vcxproj_filters_file_path)
            vcxproj_filters_file_lines = vcxproj_filters_file.readlines()
            vcxproj_filters_file.close()
            for i in xrange(len(vcxproj_filters_file_lines)):
                vcxproj_filters_file_line = vcxproj_filters_file_lines[i].strip()
                if vcxproj_filters_file_line == Filter:
                    UniqueIdentifier = vcxproj_filters_file_lines[i + 1].strip()[18:-19]
                    try: UniqueIdentifier = str(UUID(UniqueIdentifier))
                    except ValueError: return str(uuid4())
                    # print FilterInclude, UniqueIdentifier
                    return UniqueIdentifier
        except IOError:
            pass
        except:
            traceback.print_exc()

        return str(uuid4())

    def __get_filters(
        self,
        FilterHead,
        source_file_tree
    ):
        def visit_source_file(source_file):
            FilterTail = \
                ntpath(
                    relpath(
                        dirname(source_file.get_path()),
                        self.get_root_source_dir_path()
                    )
                )
            if FilterTail != "." :
                if not FilterTail.startswith('\\'): FilterTail = "\\" + FilterTail
            else: FilterTail = ""

            Filter = FilterHead + FilterTail
            Include = ntpath(source_file.get_path())

            if splitext(source_file.get_path())[1] in C_CXX_INCLUDE_FILE_EXTENSIONS:
                item_template = CL_INCLUDE_FILTERS_ITEM
            else:
                item_template = CL_COMPILE_SOURCE_FILTERS_ITEM

            return (Filter, item_template % locals())

        filters_items = \
            visit_source_file_tree(
                source_file_tree,
                visit_source_file
            )

        filters = deduplist([parts[0] for parts in filters_items])
        items = [parts[1] for parts in filters_items]

        return filters, items

    def get_include_file_tree(self):
        return self.__vcxproj.get_include_file_tree()

    def get_name(self):
        return self.__vcxproj.get_name()

    def get_project_dir_path(self):
        return self.__vcxproj.get_project_dir_path()

    def get_root_source_dir_path(self):
        return self.__vcxproj.get_root_source_dir_path()

    def get_source_file_tree(self):
        return self.__vcxproj.get_source_file_tree()

    def __replace_Filters(self, old_Filter, new_Filter, items):
        old_Filter_line = INDENT_SPACES + "<Filter>" + old_Filter + "</Filter>\n"
        new_Filter_line = INDENT_SPACES + "<Filter>" + new_Filter + "</Filter>\n"
        for item_i in xrange(len(items)):
            item_lines = StringIO(items[item_i]).readlines()
            if len(item_lines) == 3 and item_lines[1] == old_Filter_line:
                items[item_i] = "".join([item_lines[0], new_Filter_line, item_lines[2]])
        return items

    def __str__(self):
        header_filters, header_items = \
            self.__get_filters(
                "Header Files",
                self.get_include_file_tree()
        )

        source_filters, source_items = \
            self.__get_filters(
                "Source Files",
                self.get_source_file_tree()
            )

        for filters, items in \
            (
                (header_filters, header_items),
                (source_filters, source_items)
            ):

            if len(filters) == 1:
                self.__replace_Filters(filters[0], filters[0].split('\\')[0], items)
                filters[0] = filters[0].split('\\')[0]
            elif len(filters) > 1:
                filter_prefix = strlistprefix(filters).rstrip('\\')
                if len(filter_prefix) > 0:
                    for filter_i in xrange(len(filters)):
                        old_Filter = filters[filter_i]
                        new_FilterHead = old_Filter.split('\\')[0]
                        new_FilterTail = old_Filter[len(filter_prefix):].lstrip('\\')
                        if len(new_FilterTail) > 0:
                            new_Filter = new_FilterHead + '\\' + new_FilterTail
                        else:
                            new_Filter = new_FilterHead
                        filters[filter_i] = new_Filter
                        items = self.__replace_Filters(old_Filter, new_Filter, items)


                for filter in filters:
                    filter_parts = filter.split('\\')
                    for filter_part_i in xrange(1, len(filter_parts)):
                        subfilter = '\\'.join(filter_parts[0:filter_part_i])
                        if not subfilter in filters:
                            filters.append(subfilter)

        vcxproj_filters_file_path = self.get_name() + ".vcxproj.filters"
        if self.get_project_dir_path() is not None:
            vcxproj_filters_file_path = \
                join(self.get_project_dir_path(), vcxproj_filters_file_path)

        filter_items = []
        for filter in header_filters + source_filters:
            Include = filter
            UniqueIdentifier = \
                self.__get_FilterUniqueIdentifier(vcxproj_filters_file_path, Include)
            filter_items.append(FILTER_ITEM % locals())
        filter_items = indent(INDENT_SPACES * 2, "".join(filter_items))

        header_items = indent(INDENT_SPACES * 2, "".join(header_items))
        source_items = indent(INDENT_SPACES * 2, "".join(source_items))

        return """\
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
%(filter_items)s
  </ItemGroup>
  <ItemGroup>
%(header_items)s
  </ItemGroup>
  <ItemGroup>
%(source_items)s
  </ItemGroup>
</Project>
""" % locals()


class VCXProjUser(object):
    def __init__(self, *args, **kwds):
        pass

    def __str__(self):
        return """\
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
</Project>
"""


class VCXProjReference(object):
    def __init__(self, referer_dir_path, vcxproj_file_path):
        # assert exists( referer_dir_path ), referer_dir_path
        self.__referer_dir_path = referer_dir_path
        assert exists(vcxproj_file_path), vcxproj_file_path
        self.__vcxproj_file_path = vcxproj_file_path

    def get_Guid(self):
        try:
            return self.__Guid
        except AttributeError:
            vcxproj_file = open(self.__vcxproj_file_path)
            vcxproj_file_lines = vcxproj_file.readlines()
            vcxproj_file.close()
            for vcxproj_file_line in vcxproj_file_lines:
                vcxproj_file_line = vcxproj_file_line.strip()
                if vcxproj_file_line.startswith("<ProjectGuid>"):
                    Guid = vcxproj_file_line[14:-15]
                    try: Guid = str(UUID(Guid))
                    except ValueError: return str(uuid4())
                    self.__Guid = Guid
                    return Guid

            raise ValueError, "could not find GUID in " + self.__vcxproj_file_path

    def get_Include(self):
        return ntpath(relpath(self.__vcxproj_file_path, self.__referer_dir_path))

    def get_project_name(self):
        return splitext(split(self.__vcxproj_file_path)[1])[0]


class VCXProjReferences(list):
    def __init__(self, referer_dir_path, project_references):
        if isinstance(project_references, PlatformDict):
            project_references = project_references.get("win32", default=[], combine_platforms=True)

        for project_reference in project_references:
            vcxproj_file_path = join(referer_dir_path, project_reference + ".vcxproj")
            if exists(vcxproj_file_path):
                list.append(self, VCXProjReference(referer_dir_path, vcxproj_file_path))



class VCXSln(object):
    def __init__(
        self,
        project_references,
        sln_file_path=None
    ):
        if sln_file_path is not None:
            assert isinstance(sln_file_path, basestring), type(sln_file_path)
            sln_dir_path = dirname(sln_file_path)
            try:
                sln_file = open(sln_file_path)
                sln_lines = sln_file.readlines()
                sln_file.close()
                assert len(sln_lines) >= 5, len(sln_lines)
                assert sln_lines[1].strip() == "Microsoft Visual Studio Solution File, Format Version 11.00", sln_lines[1]
                assert sln_lines[2].startswith('#'), sln_lines[2]
                assert sln_lines[3].startswith('Project("{'), sln_lines[3]
                assert sln_lines[4].rstrip() == "EndProject"
                self.__Guid = sln_lines[3][10:46]
            except IOError:
                self.__Guid = str(uuid4())
        else:
            sln_dir_path = getcwd()
            self.__Guid = str(uuid4())

        self.__vcxproj_references = VCXProjReferences(sln_dir_path, project_references)

    def __str__(self):
        Projects = []
        ProjectConfigurationPlatforms = []
        for vcxproj_reference in self.__vcxproj_references:
            Projects.append(("""\
Project("{%s}") = "%s", "%s", "{%s}"
EndProject""" % (
                  self.__Guid,
                  vcxproj_reference.get_project_name(),
                  vcxproj_reference.get_Include(),
                  vcxproj_reference.get_Guid().upper()
                 )
            ).replace('\n', "\r\n"))

            ProjectConfigurationPlatforms.append(("""\
\t\t{%s}.Debug|Win32.ActiveCfg = Debug|Win32
\t\t{%s}.Debug|Win32.Build.0 = Debug|Win32
\t\t{%s}.Release|Win32.ActiveCfg = Release|Win32
\t\t{%s}.Release|Win32.Build.0 = Release|Win32""" % \
                    tuple([vcxproj_reference.get_Guid().upper()] * 4))
                .replace('\n', "\r\n")
            )
        Projects = "\r\n".join(Projects)
        ProjectConfigurationPlatforms = "\r\n".join(ProjectConfigurationPlatforms)

        return """\

Microsoft Visual Studio Solution File, Format Version 11.00
# Visual C++ Express 2010
""".replace('\n', "\r\n") + \
Projects + \
"""
Global
\tGlobalSection(SolutionConfigurationPlatforms) = preSolution
\t\tDebug|Win32 = Debug|Win32
\t\tRelease|Win32 = Release|Win32
\tEndGlobalSection
\tGlobalSection(ProjectConfigurationPlatforms) = postSolution
""".replace('\n', "\r\n") + \
ProjectConfigurationPlatforms + \
"""
\tEndGlobalSection
\tGlobalSection(SolutionProperties) = preSolution
\t\tHideSolutionNode = FALSE
\tEndGlobalSection
EndGlobal
""".replace('\n', "\r\n")


def visit_source_file_tree(
    source_file_tree,
    source_file_visitor
):
    return __visit_source_file_tree(
               source_file_tree,
               source_file_visitor,
               []
           )

def __visit_source_file_tree(
    source_file_tree,
    source_file_visitor,
    visited_source_file_paths
):
    items = []

    paths = copy(source_file_tree.keys())
    paths.sort()

    for path in paths:
        if isinstance(source_file_tree[path], dict): # path is a directory
            child_items = \
                __visit_source_file_tree(
                    source_file_tree[path],
                    source_file_visitor,
                    visited_source_file_paths
                )
            if child_items is not None:
                items.extend(child_items)
        elif path not in visited_source_file_paths:
            item = source_file_visitor(source_file_tree[path])
            if item is not None:
                items.append(item)
            visited_source_file_paths.append(path)

    return items
