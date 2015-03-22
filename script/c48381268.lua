--D・ボードン
function c48381268.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c48381268.cona)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x26))
	c:RegisterEffect(e1)
	--def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetCondition(c48381268.cond)
	e2:SetTarget(c48381268.tgd)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c48381268.cona(e)
	return e:GetHandler():IsAttackPos()
end
function c48381268.cond(e)
	return e:GetHandler():IsDefencePos()
end
function c48381268.tgd(e,c)
	return c:IsSetCard(0x26) and c~=e:GetHandler()
end
