--シャドー・インパルス
function c35329581.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCountLimit(1,35329581+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c35329581.target)
	e1:SetOperation(c35329581.activate)
	c:RegisterEffect(e1)
end
function c35329581.filter(c,e,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsCanBeEffectTarget(e)
		and c:GetPreviousControler()==tp and c:IsReason(REASON_DESTROY) and c:IsReason(REASON_BATTLE+REASON_EFFECT)
		and c:IsType(TYPE_SYNCHRO) and Duel.IsExistingMatchingCard(c35329581.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,c)
end
function c35329581.spfilter(c,e,tp,tc)
	return c:IsType(TYPE_SYNCHRO) and c:GetLevel()==tc:GetLevel()
		and c:GetRace()==tc:GetRace() and c:GetCode()~=tc:GetCode()
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c35329581.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return eg:IsContains(chkc) and c35329581.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and eg:IsExists(c35329581.filter,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=eg:FilterSelect(tp,c35329581.filter,1,1,nil,e,tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c35329581.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) or tc:IsFacedown() then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=Duel.SelectMatchingCard(tp,c35329581.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,tc)
	if sg:GetCount()>0 then
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
	end
end
