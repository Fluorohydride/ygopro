--オボミ
function c68812773.initial_effect(c)
	--summon,flip
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c68812773.sptg)
	e1:SetOperation(c68812773.spop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(68812773,0))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCost(c68812773.spcost1)
	e3:SetTarget(c68812773.sptg1)
	e3:SetOperation(c68812773.spop1)
	c:RegisterEffect(e3)
end
function c68812773.filter(c,e,tp)
	return c:IsCode(71071546) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c68812773.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c71175527.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c68812773.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c68812773.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c68812773.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK+POS_FACEDOWN_DEFENCE)
	end
end
function c68812773.costfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_MACHINE)
end
function c68812773.spfilter(c,e,tp)
	return (c:IsSetCard(0x7b)or c:IsSetCard(0x55)) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c68812773.spcost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c68812773.costfilter,1,nil) end
	local ct1=Duel.GetMatchingGroupCount(c68812773.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMST_RELEASE)
	local rg=Duel.SelectReleaseGroupEx(tp,c68812773.costfilter,1,ct1,nil)
	e:SetLabel(rg:GetCount())
	Duel.Release(rg,REASON_COST)
end
function c68812773.sptg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c68812773.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	local ct=e:GetLabel()
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,ct,0,0)
end
function c68812773.spop1(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	local ct1=Duel.GetMatchingGroupCount(c68812773.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
	if ct1<ct then ct=ct1 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local dg=Duel.SelectMatchingCard(tp,c68812773.spfilter,tp,LOCATION_HAND,0,ct,ct,nil,e,tp)
	if dg:GetCount()>0 then
		Duel.SpecialSummon(dg,0,tp,tp,false,false,POS_FACEUP)
	end
end