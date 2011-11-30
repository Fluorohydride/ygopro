--氷結界の修験者
function c20700531.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c20700531.indes)
	c:RegisterEffect(e1)
end
function c20700531.indes(e,c)
	return c:GetAttack()>=1900
end
