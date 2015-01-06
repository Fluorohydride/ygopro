--スネーク・チョーク
function c19451302.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--Battle indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(0,LOCATION_MZONE)
	e2:SetTarget(c19451302.indtg)
	e2:SetValue(aux.TargetBoolFunction(Card.IsSetCard,0x3c))
	c:RegisterEffect(e2)
end
function c19451302.indtg(e,c)
	return c:GetAttack()==0 and c:IsAttackPos()
end
