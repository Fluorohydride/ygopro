--大番狂わせ
function c32207100.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c32207100.cost)
	e1:SetTarget(c32207100.target)
	e1:SetOperation(c32207100.activate)
	c:RegisterEffect(e1)
end
function c32207100.cfilter(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsLevelBelow(2)
end
function c32207100.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c32207100.cfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c32207100.cfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c32207100.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(7) and bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)~=0 and c:IsAbleToHand()
end
function c32207100.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c32207100.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local sg=Duel.GetMatchingGroup(c32207100.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,sg,sg:GetCount(),0,0)
end
function c32207100.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(c32207100.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SendtoHand(sg,nil,REASON_EFFECT)
end
