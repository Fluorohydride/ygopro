--裁きの光
function c44595286.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetHintTiming(0,TIMING_TOHAND+0x1e0)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c44595286.condition)
	e1:SetCost(c44595286.cost)
	e1:SetTarget(c44595286.target)
	e1:SetOperation(c44595286.activate)
	c:RegisterEffect(e1)
end
function c44595286.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsEnvironment(56433456)
end
function c44595286.cfilter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c44595286.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c44595286.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c44595286.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c44595286.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_ONFIELD+LOCATION_HAND)>0 end
end
function c44595286.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,0,LOCATION_ONFIELD)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	local opt=0
	if g1:GetCount()>0 and g2:GetCount()>0 then
		opt=Duel.SelectOption(tp,aux.Stringid(44595286,0),aux.Stringid(44595286,1))+1
	elseif g1:GetCount()>0 then opt=1
	elseif g2:GetCount()>0 then opt=2
	end
	if opt==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=g1:Select(tp,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT)
	elseif opt==2 then
		Duel.ConfirmCards(tp,g2)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=g2:Select(tp,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT)
		Duel.ShuffleHand(1-tp)
	end
end
