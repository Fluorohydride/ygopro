--ロードランナー
function c36472900.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c36472900.indes)
	c:RegisterEffect(e1)
end
function c36472900.indes(e,c)
	return c:IsAttackAbove(1900)
end
