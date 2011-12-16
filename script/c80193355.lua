--救出劇
function c80193355.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BECOME_TARGET)
	e1:SetCondition(c80193355.condition)
	e1:SetTarget(c80193355.target)
	e1:SetOperation(c80193355.activate)
	c:RegisterEffect(e1)
end
function c80193355.cfilter(c)
	return c:IsLocation(LOCATION_MZONE) and c:IsFaceup() and c:IsSetCard(0x4)
end
function c80193355.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c80193355.cfilter,1,nil)
end
function c80193355.spfilter(c,e,tp,g)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false) and (not g or not g:IsContains(c))
end
function c80193355.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c80193355.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	local g=eg:Filter(c80193355.cfilter,nil)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c80193355.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()==0 then return end
	Duel.SendtoHand(g,nil,REASON_EFFECT)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=Duel.SelectMatchingCard(tp,c80193355.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp,g)
	Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
end
