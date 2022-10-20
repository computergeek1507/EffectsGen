#include "OnEffect.h"

#include "BaseProperty.h"

OnEffect::OnEffect()
{
	Properties.emplace_back(std::make_unique<IntProperty>("E_TEXTCTRL_Eff_On_Start",100));
	Properties.emplace_back(std::make_unique<IntProperty>("E_TEXTCTRL_Eff_On_End",100));
	Properties.emplace_back(std::make_unique<DoubleProperty>("E_TEXTCTRL_On_Cycles",1));
	Properties.emplace_back(std::make_unique<IntProperty>("E_TEXTCTRL_On_Transparency",0));
}