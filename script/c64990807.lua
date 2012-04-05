--氷結界の三方陣
function c64990807.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c64990807.cost)
	e1:SetTarget(c64990807.target)
	e1:SetOperation(c64990807.activate)
	c:RegisterEffect(e1)
end
function c64990807.cfilter(c)
	return c:IsSetCard(0x2f) and c:IsType(TYPE_MONSTER) and not c:IsPublic()
end
function c64990807.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetMatchingGroup(c64990807.cfilter,tp,LOCATION_HAND,0,nil):GetClassCount(Card.GetCode)>=3 end
	local g=Duel.GetMatchingGroup(c64990807.cfilter,tp,LOCATION_HAND,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local tc1=g:Select(tp,1,1,nil):GetFirst()
	g:Remove(Card.IsCode,nil,tc1:GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local tc2=g:Select(tp,1,1,nil):GetFirst()
	g:Remove(Card.IsCode,nil,tc2:GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local tc3=g:Select(tp,1,1,nil):GetFirst()
	local cg=Group.FromCards(tc1,tc2,tc3)
	Duel.ConfirmCards(1-tp,cg)
	Duel.ShuffleHand(tp)
end
function c64990807.spfilter(c,e,tp)
	return c:IsSetCard(0x2f) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c64990807.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c64990807.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c64990807.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c64990807.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
