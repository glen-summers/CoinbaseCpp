::init sparse checkout
::only way I found to get this to work is to [init, update, deinit], this leaves the module config in .git\modules\...
:: then inject the sparse config and init, update
@echo off
setlocal
cd %~dp0

git submodule init || (echo submodule init failed & exit /b 1)
git submodule update || (echo submodule update failed & exit /b 1)
git submodule deinit --all || (echo submodule deinit failed & exit /b 1)
git -C .git\modules\Dependencies\rapidjson config core.sparsecheckout true || (echo config sparse failed & exit /b 1)
echo include > .git\modules\Dependencies\rapidjson\info\sparse-checkout || (echo sparse write failed & exit /b 1)
git submodule init || (echo submodule init failed & exit /b 1)
git submodule update || (echo submodule update failed & exit /b 1)

exit /b 0