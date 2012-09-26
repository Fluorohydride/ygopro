--トライワイトゾーン
function c96383838.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c96383838.target)
	e1:SetOperation(c96383838.activate)
	c:RegisterEffect(e1)
end
function c96383838.filter(c,e,tp)
	return c:IsType(TYPE_NORMAL) and c:IsLevelBelow(2) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c96383838.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c96383838.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>2
		and Duel.IsExistingTarget(c96383838.filter,tp,LOCATION_GRAVE,0,3,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c96383838.filter,tp,LOCATION_GRAVE,0,3,3,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,3,0,0)
end
function c96383838.rfilter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c96383838.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local sg=g:Filter(c96383838.rfilter,nil,e,tp)
	if sg:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	if sg:GetCount()>ft then sg=sg:Select(tp,ft,ft,nil) end
	Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
end
