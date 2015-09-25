--ペンデュラム・コール
function c53208660.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SEARCH+CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,53208660+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c53208660.condition)
	e1:SetCost(c53208660.cost)
	e1:SetTarget(c53208660.target)
	e1:SetOperation(c53208660.activate)
	c:RegisterEffect(e1)
	Duel.AddCustomActivityCounter(53208660,ACTIVITY_CHAIN,c53208660.chainfilter)
end
function c53208660.chainfilter(re,tp,cid)
	local rc=re:GetHandler()
	local loc,seq=Duel.GetChainInfo(cid,CHAININFO_TRIGGERING_LOCATION,CHAININFO_TRIGGERING_SEQUENCE)
	return not (re:IsActiveType(TYPE_SPELL) and not re:IsHasType(EFFECT_TYPE_ACTIVATE)
		and loc==LOCATION_SZONE and (seq==6 or seq==7) and rc:IsSetCard(0x98))
end
function c53208660.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCustomActivityCount(53208660,tp,ACTIVITY_CHAIN)==0
end
function c53208660.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c53208660.thfilter(c)
	return c:IsSetCard(0x98) and c:IsType(TYPE_PENDULUM) and c:IsAbleToHand()
end
function c53208660.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c53208660.thfilter,tp,LOCATION_DECK,0,nil)
		return g:GetClassCount(Card.GetCode)>=2
	end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,2,tp,LOCATION_DECK)
end
function c53208660.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c53208660.thfilter,tp,LOCATION_DECK,0,nil)
	if g:GetClassCount(Card.GetCode)>=2 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g1=g:Select(tp,1,1,nil)
		g:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g2=g:Select(tp,1,1,nil)
		g1:Merge(g2)
		Duel.SendtoHand(g1,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g1)
	end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetTargetRange(LOCATION_SZONE,0)
	e1:SetTarget(c53208660.indtg)
	e1:SetValue(1)
	e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN)
	Duel.RegisterEffect(e1,tp)
end
function c53208660.indtg(e,c)
	return (c:GetSequence()==6 or c:GetSequence()==7) and c:IsSetCard(0x98)
end
