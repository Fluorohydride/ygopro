--翻弄するエルフの剣士
function c52077741.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c52077741.indes)
	c:RegisterEffect(e1)
end
function c52077741.indes(e,c)
	return c:GetAttack()>=1900
end
