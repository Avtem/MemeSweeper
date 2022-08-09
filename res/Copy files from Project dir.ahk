SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

loop % 2
{
	if(a_index == 1)
		destName  := "img"
	else
		destName  := "snd"
		
	srcFolder := "V:\0010\activeProjects\Visual Studio\Meme Sweeper\Engine\" . destName


	if !fileExist(srcFolder) {
		msgbox 16, % "Can't back up", % "Oh-oh! There is nothing to back up... is the app/game installed?"
		return
	}

	fileRemoveDir %destName%, true
	fileCopyDir %srcFolder%, %destName%

	if ErrorLevel
		msgBox 16, % "Can't back up",  "The folder did not copy. Some mysterious thing happened!"
}