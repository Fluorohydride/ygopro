--イグナイト・アヴェンジャー
function c23296404.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c23296404.sptg)
	e1:SetOperation(c23296404.spop)
	c:RegisterEffect(e1)
	--todeck
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_TODECK)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetTarget(c23296404.tdtg)
	e2:SetOperation(c23296404.tdop)
	c:RegisterEffect(e2)
end
function c23296404.desfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xc8) and c:IsDestructable()
end
function c23296404.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(tp) and c23296404.desfilter(chkc) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingTarget(c23296404.desfilter,tp,LOCATION_ONFIELD,0,3,nil) end
	local g=Duel.SelectTarget(tp,c23296404.desfilter,tp,LOCATION_ONFIELD,0,3,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c23296404.spop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if Duel.Destroy(g,REASON_EFFECT)~=0 then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c23296404.thfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xc8) and c:IsAbleToHand()
end
function c23296404.tdfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToDeck()
end
function c23296404.tdtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c23296404.thfilter(chkc) and chkc~=e:GetHandler() end
	if chk==0 then return Duel.IsExistingTarget(c23296404.thfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
		and Duel.IsExistingMatchingCard(c23296404.tdfilter,tp,0,LOCATION_ONFIELD,1,nil) end
	local g=Duel.SelectTarget(tp,c23296404.thfilter,tp,LOCATION_MZONE,0,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,1,1-tp,LOCATION_ONFIELD)
end
function c23296404.tdop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
	local g=Duel.GetMatchingGroup(c23296404.tdfilter,tp,0,LOCATION_ONFIELD,nil)
	if tc:IsLocation(LOCATION_HAND+LOCATION_EXTRA) and g:GetCount()>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local rg=g:Select(tp,1,1,nil)
		Duel.SendtoDeck(rg,nil,1,REASON_EFFECT)
	end
end
