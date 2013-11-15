--森羅の実張り ピース
function c63257623.initial_effect(c)
	--deck check
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(63257623,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c63257623.target)
	e1:SetOperation(c63257623.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(63257623,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY+EFFECT_FLAG_CHAIN_UNIQUE)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCondition(c63257623.spcon)
	e3:SetCost(c63257623.spcost)
	e3:SetTarget(c63257623.sptg)
	e3:SetOperation(c63257623.spop)
	c:RegisterEffect(e3)
	if not c63257623.global_check then
		c63257623.global_check=true
		c63257623[0]=Group.CreateGroup()
		c63257623[0]:KeepAlive()
		c63257623[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CONFIRM_DECKTOP)
		ge1:SetOperation(c63257623.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c63257623.checkop(e,tp,eg,ep,ev,re,r,rp)
	c63257623[0]:Clear()
	c63257623[0]:Merge(eg)
	c63257623[1]=re
end
function c63257623.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0 end
end
function c63257623.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	Duel.ConfirmDecktop(tp,1)
	local g=Duel.GetDecktopGroup(tp,1)
	local tc=g:GetFirst()
	if tc:IsRace(RACE_PLANT) then
		Duel.DisableShuffleCheck()
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		Duel.MoveSequence(tc,1)
	end
end
function c63257623.spcon(e,tp,eg,ep,ev,re,r,rp)
	return re and e:GetHandler():IsPreviousLocation(LOCATION_DECK)
		and c63257623[0]:IsContains(e:GetHandler()) and c63257623[1]==re
end
function c63257623.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,63257623)==0 end
	Duel.RegisterFlagEffect(tp,63257623,RESET_PHASE+PHASE_END,0,1)
end
function c63257623.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsRace(RACE_PLANT) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c63257623.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c63257623.filter(chkc,e,tp) end
	if chk==0 then return Duel.IsExistingTarget(c63257623.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c63257623.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c63257623.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
