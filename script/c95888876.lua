--ヘルカイザー・ドラゴン
function c95888876.initial_effect(c)
	aux.EnableDualAttribute(c)
	--double atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(1)
	e1:SetCondition(aux.IsDualState)
	c:RegisterEffect(e1)
end