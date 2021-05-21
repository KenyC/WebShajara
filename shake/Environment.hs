{-# LANGUAGE RecordWildCards #-}
module Environment where

import System.Console.GetOpt
--
import Control.Monad
--
import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util



options :: ShakeOptions
options = shakeOptions {
    shakeFiles = ".shake",
    shakeColor = True
}

data Flag = RunExe | Debug | Valgrind | Gdb deriving (Eq, Show)
flags :: [OptDescr (Either String Flag)]
flags = [ Option "-r" ["--run"]         (NoArg $ Right RunExe)      "run executable"   
        , Option "-d" ["--debug"]       (NoArg $ Right Debug)       "compile in debug" 
        , Option "-g" ["--gdb"]         (NoArg $ Right Gdb)         "run executable with gdb" 
        , Option "-v" ["--valgrind"]    (NoArg $ Right Valgrind)    "run executable in valgrind" ]
-- flags = []

data Environment = Environment {
    run         :: FilePath -> Action (),
    extra_flags :: [String],
    debug       :: Bool,
    runExe      :: Bool
}

compileFlags :: [Flag] -> Environment
compileFlags flags = let
    valgrind = Valgrind `elem` flags
    gdb      = Gdb      `elem` flags
    debug    = Debug    `elem` flags || valgrind || gdb
    runExe   = RunExe   `elem` flags || valgrind || gdb
    run  file = case (valgrind, gdb) of
        (True,  True)  ->  error "Cannot both run GDB & Valgrind"
        (True,  False) ->  cmd_ "valgrind" "--leak-check=yes" file
        (False, True)  ->  cmd_ "gdb" file
        (False, False) ->  cmd_ file

    extra_flags = if debug 
                  then ["-g", "-O0"]
                  else [] 
    in Environment{..}



shakeArgsPassOptions :: ShakeOptions -> ([Flag] -> Rules ()) -> IO ()
shakeArgsPassOptions options rules = shakeArgsWith options flags $ \flags targets -> return $ Just $ do
    if null targets then rules flags else want targets >> withoutActions (rules flags)

