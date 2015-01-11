--デュアル・ランサー
function c26254876.initial_effect(c)
	aux.EnableDualAttribute(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetCondition(aux.IsDualState)
	c:RegisterEffect(e1)
end
