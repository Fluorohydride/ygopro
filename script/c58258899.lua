--TGX300
function c58258899.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atk up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetValue(c58258899.val)
	c:RegisterEffect(e2)
end
function c58258899.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x27)
end
function c58258899.val(e,c)
	return Duel.GetMatchingGroupCount(c58258899.filter,c:GetControler(),LOCATION_MZONE,0,nil)*300
end
