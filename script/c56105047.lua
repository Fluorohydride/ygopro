--ゴゴゴゴースト
function c56105047.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(56105047,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCountLimit(1,56105047)
	e1:SetTarget(c56105047.sptg)
	e1:SetOperation(c56105047.spop)
	c:RegisterEffect(e1)
end
function c56105047.filter(c,e,tp)
	return c:GetCode()==62476815 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c56105047.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c56105047.filter(chkc,e,tp) end
	if chk==0 then return Duel.IsExistingTarget(c56105047.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c56105047.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c56105047.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_DEFENCE)==1 then
		if c:IsPosition(POS_FACEUP_ATTACK) and c:IsRelateToEffect(e) then
			Duel.BreakEffect()
			Duel.ChangePosition(c,POS_FACEUP_DEFENCE)
		end
	end
end
