--レベルダウン！？
function c90500169.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TODECK)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c90500169.target)
	e1:SetOperation(c90500169.activate)
	c:RegisterEffect(e1)
end
function c90500169.filter(c,e,tp)
	if c:IsFacedown() or not c:IsSetCard(0x41) or not c:IsAbleToDeck() then return false end
	local code=c:GetCode()
	local class=_G["c"..code]
	return class and class.lvdncount~=nil and Duel.IsExistingMatchingCard(c90500169.spfilter,tp,LOCATION_GRAVE,0,1,nil,class,e,tp)
end
function c90500169.spfilter(c,class,e,tp)
	local code=c:GetCode()
	for i=1,class.lvdncount do
		if code==class.lvdn[i] then	return c:IsCanBeSpecialSummoned(e,0,tp,true,true) end
	end
	return false
end
function c90500169.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c90500169.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingTarget(c90500169.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c90500169.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
end
function c90500169.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local code=tc:GetCode()
	if not tc:IsRelateToEffect(e) then return end
	local rc=Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
	if tc:GetLocation()==LOCATION_DECK then Duel.ShuffleDeck(tc:GetControler()) end
	if rc==0 then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local class=_G["c"..code]
	if class==nil or class.lvdncount==nil then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c90500169.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,class,e,tp)
	local tc=g:GetFirst()
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,true,true,POS_FACEUP)
	end
end
