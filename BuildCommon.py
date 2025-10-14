from SCons.Script import Glob

def GetSources(env):
    env.Append(CPPPATH=["src/"])
    sources = Glob("src/*.cpp")
    return sources