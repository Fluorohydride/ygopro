--ラストバトル！
function c28566710.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_STANDBY_PHASE)
	e1:SetCondition(c28566710.condition)
	e1:SetTarget(c28566710.target)
	e1:SetOperation(c28566710.operation)
	c:RegisterEffect(e1)
end
function c28566710.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<=1000 and Duel.GetTurnPlayer()~=tp
end
function c28566710.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(nil,tp,LOCATION_MZONE,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,1-tp,LOCATION_DECK)
end
function c28566710.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c28566710.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local tg=Duel.SelectMatchingCard(tp,nil,tp,LOCATION_MZONE,0,1,1,nil)
	local tc=tg:GetFirst()
	local hg=Duel.GetFieldGroup(tp,0xe,0xe)
	if tc then hg:RemoveCard(tc) end
	Duel.SendtoGrave(hg,REASON_EFFECT)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(1-tp,c28566710.spfilter,1-tp,LOCATION_DECK,0,1,1,nil,e,tp)
	local sc=g:GetFirst()
	if sc then
		Duel.BreakEffect()
		Duel.SpecialSummon(sc,0,1-tp,1-tp,false,false,POS_FACEUP_ATTACK)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
		e1:SetTargetRange(1,1)
		e1:SetReset(RESET_CHAIN)
		Duel.RegisterEffect(e1,tp)
		if tc then Duel.CalculateDamage(sc,tc) end
	end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetOperation(c28566710.checkop)
	Duel.RegisterEffect(e1,tp)
end
function c28566710.checkop(e,tp,eg,ep,ev,re,r,rp)
	local t1=Duel.GetFieldGroupCount(0,LOCATION_MZONE,0)
	local t2=Duel.GetFieldGroupCount(1,LOCATION_MZONE,0)
	if t1>0 and t2==0 then
		Duel.Win(0,0x16)
	elseif t2>0 and t1==0 then
		Duel.Win(1,0x16)
	else
		Duel.Win(PLAYER_NONE,0x16)
	end
end
