--グレイドル・パラサイト
function c49966595.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_ATTACK)
	e1:SetTarget(c49966595.acttg)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,49966595)
	e2:SetCondition(c49966595.spcon1)
	e2:SetCost(c49966595.spcost1)
	e2:SetTarget(c49966595.sptg1)
	e2:SetOperation(c49966595.spop1)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCountLimit(1,49966596)
	e3:SetCondition(c49966595.spcon2)
	e3:SetCost(c49966595.spcost2)
	e3:SetTarget(c49966595.sptg2)
	e3:SetOperation(c49966595.spop2)
	c:RegisterEffect(e3)
end
function c49966595.acttg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return c49966595.sptg2(e,tp,eg,ep,ev,re,r,rp,0,chkc) end
	if chk==0 then return true end
	if Duel.CheckEvent(EVENT_ATTACK_ANNOUNCE) then
		if c49966595.spcon1(e,tp,eg,ep,ev,re,r,rp)
			and c49966595.spcost1(e,tp,eg,ep,ev,re,r,rp,0)
			and c49966595.sptg1(e,tp,eg,ep,ev,re,r,rp,0)
			and Duel.SelectYesNo(tp,aux.Stringid(49966595,0)) then
			e:SetCategory(CATEGORY_SPECIAL_SUMMON)
			e:SetProperty(0)
			e:SetOperation(c49966595.spop1)
			c49966595.spcost1(e,tp,eg,ep,ev,re,r,rp,1)
			c49966595.sptg1(e,tp,eg,ep,ev,re,r,rp,1)
			return
		elseif c49966595.spcon2(e,tp,eg,ep,ev,re,r,rp)
			and c49966595.spcost2(e,tp,eg,ep,ev,re,r,rp,0)
			and c49966595.sptg2(e,tp,eg,ep,ev,re,r,rp,0,chkc)
			and Duel.SelectYesNo(tp,aux.Stringid(49966595,0)) then
			e:SetCategory(CATEGORY_SPECIAL_SUMMON)
			e:SetProperty(EFFECT_FLAG_CARD_TARGET)
			e:SetOperation(c49966595.spop2)
			c49966595.spcost2(e,tp,eg,ep,ev,re,r,rp,1)
			c49966595.sptg2(e,tp,eg,ep,ev,re,r,rp,1,chkc)
			return
		end
	end
	e:SetCategory(0)
	e:SetProperty(0)
	e:SetOperation(nil)
end
function c49966595.spcon1(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttacker():IsControler(1-tp) and Duel.GetAttackTarget()==nil
end
function c49966595.spcost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,49966595)==0 end
	Duel.RegisterFlagEffect(tp,49966595,RESET_PHASE+PHASE_END,0,1)
end
function c49966595.spfilter1(c,e,tp)
	return c:IsSetCard(0xd1) and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP)
end
function c49966595.sptg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c49966595.spfilter1,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c49966595.spop1(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)~=0 then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c49966595.spfilter1,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_ATTACK)
	end
end
function c49966595.spcon2(e,tp,eg,ep,ev,re,r,rp)
	local c=Duel.GetAttacker()
	return c:IsControler(tp) and c:IsSetCard(0xd1) and Duel.GetAttackTarget()==nil
end
function c49966595.spcost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,49966596)==0 end
	Duel.RegisterFlagEffect(tp,49966596,RESET_PHASE+PHASE_END,0,1)
end
function c49966595.spfilter2(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp)
end
function c49966595.sptg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_GRAVE) and c49966595.spfilter2(chkc,e,tp) end
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)==0
		and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c49966595.spfilter2,tp,0,LOCATION_GRAVE,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c49966595.spfilter2,tp,0,LOCATION_GRAVE,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c49966595.spop2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)~=0 then return end
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,1-tp,false,false,POS_FACEUP)
	end
end
