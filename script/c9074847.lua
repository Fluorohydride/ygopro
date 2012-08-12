--大騒動
function c9074847.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCondition(c9074847.condition)
	e1:SetTarget(c9074847.target)
	e1:SetOperation(c9074847.operation)
	c:RegisterEffect(e1)
end
function c9074847.confilter(c,tp)
	return c:IsControler(tp) and c:IsPreviousLocation(LOCATION_MZONE)
end
function c9074847.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and eg:IsExists(c9074847.confilter,1,nil,tp)
end
function c9074847.thfilter(c,tp)
	return c:IsLocation(LOCATION_HAND) and c:IsControler(tp)
end
function c9074847.spfilter(c,e,tp)
	return c:GetLevel()<=4 and c:IsSummonableCard() and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c9074847.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c9074847.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	Duel.SendtoHand(g,nil,REASON_EFFECT)
	Duel.BreakEffect()
	local og=Duel.GetOperatedGroup()
	local ct1=og:FilterCount(c9074847.thfilter,nil,tp)
	local ct2=og:FilterCount(c9074847.thfilter,nil,1-tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g1=Duel.SelectMatchingCard(tp,c9074847.spfilter,tp,LOCATION_HAND,0,ct1,ct1,nil,e,tp)
	if g1:GetCount()>0 then
		local tc=g1:GetFirst()
		while tc do
			Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
			tc=g1:GetNext()
		end
	end
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local g2=Duel.SelectMatchingCard(1-tp,c9074847.spfilter,1-tp,LOCATION_HAND,0,ct2,ct2,nil,e,1-tp)
	if g2:GetCount()>0 then
		local tc=g2:GetFirst()
		while tc do
			Duel.SpecialSummonStep(tc,0,1-tp,1-tp,false,false,POS_FACEDOWN_DEFENCE)
			tc=g2:GetNext()
		end
	end
	Duel.SpecialSummonComplete()
end
