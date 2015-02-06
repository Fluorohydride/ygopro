--陽気な葬儀屋
function c41142615.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c41142615.target)
	e1:SetOperation(c41142615.activate)
	c:RegisterEffect(e1)
end
function c41142615.filter(c)
	return c:IsType(TYPE_MONSTER)
end
function c41142615.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c41142615.filter,tp,LOCATION_HAND,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
end
function c41142615.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardHand(tp,c41142615.filter,1,3,REASON_EFFECT+REASON_DISCARD)
end
