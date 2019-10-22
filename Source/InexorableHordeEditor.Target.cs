// Bradley Olson - GNU General Public License v3.0

using UnrealBuildTool;
using System.Collections.Generic;

public class InexorableHordeEditorTarget : TargetRules
{
	public InexorableHordeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "InexorableHorde" } );
	}
}
