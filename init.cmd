:: init sparse checkout
:: one way I found to get sparse submodule checkout to work is to [init, update, deinit], this leaves the module config in .git\modules\... then inject the sparse config and rerun [init, update]
:: an alternative is to update --init with a bogus cmd this creates the .git module info but no update, then set the sparse data and then do a real update
@echo off
setlocal
cd %~dp0

git -c submodule.Dependencies/rapidjson.update=^!echo submodule update --init || (echo submodule update failed & exit /b 1)
git -C .git\modules\Dependencies\rapidjson config core.sparsecheckout true || (echo config sparse failed & exit /b 1)
echo include > .git\modules\Dependencies\rapidjson\info\sparse-checkout || (echo sparse write failed & exit /b 1)
git submodule update --init || (echo submodule update failed & exit /b 1)

exit /b 0