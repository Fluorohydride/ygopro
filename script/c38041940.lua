--シード・オブ・フレイム
function c38041940.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(38041940,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c38041940.condition)
	e1:SetTarget(c38041940.target)
	e1:SetOperation(c38041940.operation)
	c:RegisterEffect(e1)
end
function c38041940.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return bit.band(c:GetReason(),0x41)==0x41 and c:IsPreviousLocation(LOCATION_ONFIELD)
		and c:GetPreviousControler()==tp
end
function c38041940.spfilter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsRace(RACE_PLANT) and c:GetCode()~=38041940 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c38041940.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c38041940.spfilter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c38041940.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		and Duel.IsPlayerCanSpecialSummonMonster(tp,38041941,0,0x4011,0,0,1,RACE_PLANT,ATTRIBUTE_EARTH,POS_FACEUP_DEFENCE,1-tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c38041940.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,tp,0)
end
function c38041940.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP) then
		if Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
			and Duel.IsPlayerCanSpecialSummonMonster(tp,38041941,0,0x4011,0,0,1,RACE_PLANT,ATTRIBUTE_EARTH,POS_FACEUP_DEFENCE,1-tp) then
			local token=Duel.CreateToken(tp,38041941)
			Duel.SpecialSummonStep(token,0,tp,1-tp,false,false,POS_FACEUP_DEFENCE)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UNRELEASABLE_SUM)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			e1:SetValue(1)
			token:RegisterEffect(e1,true)
		end
	end
	Duel.SpecialSummonComplete()
end
