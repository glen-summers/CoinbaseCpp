::init sparse checkout
@echo off
setlocal
cd %~dp0

git config core.sparsecheckout true || (echo git config failed & exit /b 1)
git submodule init || (echo submodule init & exit /b 1)

if not exist .git\modules\Dependencies\rapidjson\info md .git\modules\Dependencies\rapidjson\info || (echo md failed & exit /b 1)

echo include > .git\modules\Dependencies\rapidjson\info\sparse-checkout || (echo sparse write failed & exit /b 1)
git read-tree -mu HEAD || (echo git read tree failed & exit /b 1)

exit /b 0