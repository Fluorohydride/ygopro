--堕天使スペルビア
function c11260714.initial_effect(c)
	--spsum success
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(11260714,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c11260714.spcon)
	e1:SetTarget(c11260714.sptg)
	e1:SetOperation(c11260714.spop)
	c:RegisterEffect(e1)
end
function c11260714.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_GRAVE)
end
function c11260714.filter(c,e,sp)
	return c:IsRace(RACE_FAIRY) and c:GetCode()~=11260714 and c:IsCanBeSpecialSummoned(e,0,sp,false,false)
end
function c11260714.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c11260714.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c11260714.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c11260714.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c11260714.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
