--コンタクト・アウト
function c69270537.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c69270537.target)
	e1:SetOperation(c69270537.activate)
	c:RegisterEffect(e1)
end
function c69270537.tdfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x9) and c:IsType(TYPE_FUSION) and c:IsAbleToExtra()
end
function c69270537.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c69270537.tdfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c69270537.tdfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c69270537.tdfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c69270537.spfilter(c,e,tp,fc)
	for i=1,fc.material_count do
		if c:GetCode()==fc.material[i] then return c:IsCanBeSpecialSummoned(e,0,tp,false,false) end
	end
	return false
end
function c69270537.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)~=0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>1 and tc:IsLocation(LOCATION_EXTRA) then
		local sg=Duel.GetMatchingGroup(c69270537.spfilter,tp,LOCATION_DECK,0,nil,e,tp,tc)
		if tc:CheckFusionMaterial(sg,nil,PLAYER_NONE) and Duel.SelectYesNo(tp,aux.Stringid(69270537,0)) then
			Duel.BreakEffect()
			local mats=Duel.SelectFusionMaterial(tp,tc,sg,nil,PLAYER_NONE)
			Duel.SpecialSummon(mats,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
