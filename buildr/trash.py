class BuildItem:
    def goDepTar(self, tar, level):
        self.buildr.logItemGoDepTar(self, tar, level)
        dep = self.buildr.itemGetNew(tar)
        return self.goDep(dep, level)
        
class MyBuildExt(_st_build_ext):
    def run(self):
        print(f'ZYX MyBuildExt.run')
        return super().run()
    
    def build_extension(self, ext):
        print(f'ZYX MyBuildExt.build_extension')
        #dump1('ext', ext)

class MyBuildMetaBackend(_BuildMetaBackend):
    def build_wheel(self, wheel_directory, config_settings=None, metadata_directory=None):
        print(f'ZYX build_wheel 0 {wheel_directory=!r} {config_settings=!r} {metadata_directory=!r}')
        ret = super().build_wheel(wheel_directory, config_settings, metadata_directory)
        print(f'ZYX build_wheel 1 {wheel_directory=!r} {config_settings=!r} {metadata_directory=!r} = {ret!r}')
        return ret

    def build_sdist(self, sdist_directory, config_settings=None):
        print(f'ZYX build_sdist 0 {sdist_directory=!r} {config_settings=!r}')
        ret = super().build_sdist(sdist_directory, config_settings)
        print(f'ZYX build_sdist 1 {sdist_directory=!r} {config_settings=!r} = {ret!r}')
        return ret
