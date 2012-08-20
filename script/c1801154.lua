--遠心分離フィールド
function c1801154.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(1801154,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetTarget(c1801154.sptg)
	e2:SetOperation(c1801154.spop)
	c:RegisterEffect(e2)
end
function c1801154.filter2(c,code)
	if not c.material_count or not c:IsReason(REASON_DESTROY) or not c:IsReason(REASON_EFFECT) then return false end
	for i=1,c.material_count do
		if code==c.material[i] then return true end
	end
	return false
end
function c1801154.filter1(c,e,tp,eg)
	if not c:IsCanBeSpecialSummoned(e,0,tp,false,false) then return false end
	return eg:IsExists(c1801154.filter2,1,nil,c:GetCode())
end
function c1801154.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c1801154.filter1,tp,LOCATION_GRAVE,0,1,nil,e,tp,eg) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c1801154.filter1,tp,LOCATION_GRAVE,0,1,1,nil,e,tp,eg)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c1801154.spop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
