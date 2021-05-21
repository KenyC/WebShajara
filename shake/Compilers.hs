module Compilers where
import Control.Monad
import Text.Printf
--
import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util


oFilesFor :: FilePath -> Action [FilePath]
oFilesFor directory = do
    cFiles <- getDirectoryFiles "." [directory </> "*.c"]
    return $ map (-<.> "o") $ cFiles


libraries :: [String]
libraries = ["cairo"]

includes :: [String]
includes = ["/usr/include/cairo"]



gcc_flags_default :: [String]
gcc_flags_default = join $ [
    ["-fdiagnostics-color=always"],
    ["-I" ++ include | include <- includes],
    ["-l" ++ lib     | lib     <- libraries]]



gcc_c :: String   -- C file
      -> String   -- O file
      -> String   -- M file (dependency file)
      -> [String] -- extra flags
      -> Action ()
gcc_c c_file o_file m_file extra_flags = cmd_ "gcc" "-c" c_file "-o" o_file gcc_flags_default extra_flags ["-MMD", "-MF"] m_file

gcc :: [String] -- O files 
    -> String   -- output exe file
    -> [String] -- extra flags
    -> Action ()
gcc  o_files exe_file extra_flags = cmd_ "gcc" o_files "-o" exe_file gcc_flags_default extra_flags



emcc_flags_default :: [String]
emcc_flags_default = join $ [
    ["-fdiagnostics-color=always"],
    ["-O2", "-DWEB", "-s", "WASM=1"], 
    ["-s", "EXTRA_EXPORTED_RUNTIME_METHODS=[cwrap,allocate,intArrayFromString]"]
    ]



emcc_c :: String   -- C file
       -> String   -- O file
       -> String   -- M file (dependency file)
       -> [String] -- extra flags
       -> Action ()
emcc_c c_file o_file m_file extra_flags = cmd_ "emcc" "-c" c_file "-o" o_file emcc_flags_default extra_flags ["-MMD", "-MF"] m_file

emcc :: [String] -- O files 
     -> String   -- output exe file
     -> [String] -- extra flags
     -> Action ()
emcc o_files exe_file extra_flags = cmd_ "emcc" 
                                         o_files 
                                         "-o" exe_file 
                                         emcc_flags_default extra_flags 
                                         


