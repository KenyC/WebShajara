{-# LANGUAGE TypeOperators   #-}
{-# LANGUAGE RecordWildCards #-}
import System.Console.GetOpt
--
import Control.Monad
--
import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util

import Compilers
import Environment


main :: IO ()
main = shakeArgsPassOptions options $ \flags -> do
    let env@Environment{..} = compileFlags flags

    
    want ["main"]




    phony "main" $ do
        need ["release" </> "main.html", 
              "release" </> "main.js", 
              "release" </> "main.wasm", 
              "release" </> "main.css"]



    ["release" </> "*.html", "release" </> "*.css"] |%> \out -> do
        let original = "src" </> "html"  </> (takeFileName out)
        need [original]
        cmd_ "cp" "-f" original out


    ["release" </> "main.js", "release" </> "main.wasm"] &%> \[jsFile, wasmFile] -> do
        oFiles <- oFilesFor "src/c"
        putInfo $ show oFiles
        need oFiles
        emcc oFiles jsFile extra_flags

    "src" </> "c" </> "*.o" %> \out -> do
            let cFile = out -<.> "c"
                mFile = "build" </> out -<.> "m"
                -- mFile = out -<.> "m"
            need [cFile]
            quietly $ cmd_ "mkdir" "-p" (takeDirectory mFile)
            emcc_c cFile out mFile extra_flags
            neededMakefileDependencies mFile

    -------------------------------------------
    ------------------- TESTS -----------------
    -------------------------------------------

    ------------------- RULES COMMON TO ALL TESTS -----------------

    "build" </> "test" </> "*.o" %> \out -> do
        let cFile = "src" </> "c" </> (takeBaseName out) <.> "c"
            mFile = "build" </> cFile -<.> "m"

        need [cFile]
        quietly $ cmd_ "mkdir" "-p" (takeDirectory mFile)
        gcc_c cFile out mFile $ ("-Isrc" </> "c"):extra_flags   
        neededMakefileDependencies mFile

    "test//*.o" %> \out -> do
        let cFile = out -<.> "c"
            mFile = "build" </> cFile -<.> "m"

        need [cFile]
        quietly $ cmd_ "mkdir" "-p" (takeDirectory mFile)
        gcc_c cFile out mFile $ ("-Isrc" </> "c"):extra_flags   
        neededMakefileDependencies mFile

    ------------------- TEST GEOMETRY -----------------

    phony "geometry" $ do
        let exeFile = "build" </> "geometry" 
        need [exeFile]
        when runExe $ run exeFile

    "build" </> "geometry" %> \out -> do
        let originalCFiles = ["geometry.c"]
            originalOFiles = ["build" </> "test" </> originalCFile -<.> "o" | originalCFile <- originalCFiles]
        testOFiles  <- oFilesFor $ "test" </> "geometry_src"

        let allOFiles = originalOFiles ++ testOFiles
        need allOFiles
        gcc allOFiles out extra_flags




    ------------------- TEST TREE -----------------

    phony "tree" $ do
        let exeFile = "build" </> "tree" 
        need [exeFile]
        when runExe $ run exeFile

    "build" </> "tree" %> \out -> do
        let originalCFiles = ["tree.c",  "tree_output.c", "tree_parse.c", "geometry.c"]
            originalOFiles = ["build" </> "test" </> originalCFile -<.> "o" | originalCFile <- originalCFiles]

        testOFiles  <- oFilesFor $ "test" </> "tree_src"

        let allOFiles = originalOFiles ++ testOFiles
        need allOFiles
        gcc allOFiles out extra_flags




    priority 2 $ "test" </> "tree_src" </> "output" <.> "o" %> \out -> do
        rawDataFiles <- getDirectoryFiles "." ["test" </> "tree_src" </> "data" </> "*.data"]
        let cFile      = out -<.> "c"
            mFile      = "build" </> cFile -<.> "m"
            dataFiles  = [file <.> "txt" | file <- rawDataFiles]
        need $ cFile:dataFiles
        quietly $ cmd_ "mkdir" "-p" (takeDirectory mFile)
        gcc_c cFile out mFile $ ("-Isrc" </> "c"):extra_flags   
        neededMakefileDependencies mFile

    "test" </> "tree_src" </> "data" </> "*.txt" %> \out -> do
        let Just rawDataFile = stripExtension "txt" out  
        need [rawDataFile]
        cmd_ 
            "python" 
            ("scripts" </> "sanitize_tree.py")
            rawDataFile


    ------------------- ADDITIONAL PHONIES -----------------
    phony "view" $ do
        cmd_ "firefox" "http://0.0.0.0:8000/build/main.html"

    phony "clean" $ do
        removeFilesAfter "release"      ["*"]
        removeFilesAfter "src"          ["*.o"]
        removeFilesAfter "build"        ["*"]
