--妖仙郷の眩暈風
function c62681049.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c62681049.condition)
	c:RegisterEffect(e1)
	--todeck
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_TO_HAND_REDIRECT)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c62681049.tdtg)
	e2:SetCondition(c62681049.tdcon)
	e2:SetValue(LOCATION_DECKSHF)
	c:RegisterEffect(e2)
end
function c62681049.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(6) and c:IsSetCard(0xb3)
end
function c62681049.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c62681049.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c62681049.tdtg(e,c)
	return (c:IsFacedown() or not c:IsSetCard(0xb3)) and c:IsReason(REASON_EFFECT)
end
function c62681049.tdcon(e)
	local tp=e:GetHandlerPlayer()
	local tc1=Duel.GetFieldCard(tp,LOCATION_SZONE,6)
	local tc2=Duel.GetFieldCard(tp,LOCATION_SZONE,7)
	return (tc1 and tc1:IsSetCard(0xb3)) or (tc2 and tc2:IsSetCard(0xb3))
end
