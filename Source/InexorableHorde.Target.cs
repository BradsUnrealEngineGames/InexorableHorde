// Bradley Olson - GNU General Public License v3.0

using UnrealBuildTool;
using System.Collections.Generic;

public class InexorableHordeTarget : TargetRules
{
	public InexorableHordeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "InexorableHorde" } );
	}
}
