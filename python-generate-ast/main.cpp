/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file main.cpp
/// \brief Generates python ast stuff from header
/**
 * Program accepts definition file and produces:
 *  1. astnodes.h header
 *  2. astnodes.h implementation
 *  3. ITreeVisitor interface in itreevisitor.h
 *  4. IdleTreeVisitor subclass of ITreeVisitor in idletreevisitor.h
 *  5. AstKind enum in astkind.h
 */

#include "scopedclangoptions.h"
#include "generator.h"
#include "generators/headergen.h"
#include "generators/pwdgen.h"
#include "generators/visitorgen.h"
#include "generators/implementationgen.h"
#include "generators/astkindsgen.h"
#include "generators/fullvisitorgen.h"

#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

void checkExistance(const std::string &path)
{
    if (access(path.c_str(), R_OK) != 0)
    {
        printf("Wrong path: %s\n", path.c_str());
        exit(1);
    }
}

void changeDirectory(const std::string &path)
{
    checkExistance(path);
    int status = chdir(path.c_str());
    if (status != 0)
    {
        printf("Directory exists, but we cannot goto it: %s\n",
               path.c_str());
        exit(1);
    }
}

void writeStringToFile(const std::string &content, const std::string &path)
{
    FILE *file = fopen(path.c_str(), "w");
    if (file == nullptr) {
        printf("Cannot open AST implementation file: %s\n", path.c_str());
        exit(0);
    }

    fwrite(content.c_str(), sizeof(char), content.size(), file);
    fclose(file);
}

template<class TGenerator>
void runAndWrite(const std::string &fileName, CXTranslationUnit unit)
{
    Generator *gen = new TGenerator();
    gen->init(unit, "ast.def.h", fileName);
    gen->run();
    writeStringToFile(gen->runResult(), fileName);
    delete gen;
}

int main(int argc, char *argv[])
{
    if (argc > 2) {
        puts("Usage: GenerateAST <workdir>");
        exit(0);
    }
    const std::string dir(argv[1]);
    const std::string path("ast.def.h");
    changeDirectory(dir);
    checkExistance(path);

    std::vector<std::string> optionsArray = {
        "-xc++",
        "-D__STDC_LIMIT_MACROS",
        "-D__STDC_CONSTANT_MACROS",
        "-DPYTHONAST_AST_GENERATOR",
        "-I/usr/include/x86_64-linux-gnu/c++/4.7/",
        "-I/usr/include/qt4",
        "-I/usr/include/qt4/QtCore",
        "-I/usr/lib/clang/3.2/include",
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Wdocumentation"
    };
    ScopedClangOptions options(optionsArray);

    CXIndex index = clang_createIndex(false /*bool exclude PCH decls*/,
                                      true /*bool display diagnostic*/);

    CXTranslationUnit unit = clang_parseTranslationUnit(
                index,
                path.c_str(),
                options.data(), /* argv */
                options.size(), /* argc */
                nullptr, /* unsaved files */
                0, /* num unsaved files */
                CXTranslationUnit_DetailedPreprocessingRecord
                );

    runAndWrite<HeaderGen>("astnodes.h", unit);
    runAndWrite<PwdGen>("ast_pwd.h", unit);
    runAndWrite<VisitorGen>("itreevisitor.h", unit);
    runAndWrite<FullVisitorGen>("fulltreevisitor.h", unit);
    runAndWrite<ImplementationGen>("astnodes.cpp", unit);
    runAndWrite<ASTKindsGen>("astkind.h", unit);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return 0;
}

