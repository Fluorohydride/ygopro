--フォーチュン・インハーリット
function c20057949.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c20057949.condition)
	e1:SetOperation(c20057949.activate)
	c:RegisterEffect(e1)
	if not c20057949.global_check then
		c20057949.global_check=true
		c20057949[0]=false
		c20057949[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DESTROY)
		ge1:SetOperation(c20057949.checkop1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c20057949.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c20057949.checkop1(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		local pos=tc:GetPosition()
		if Duel.GetCurrentPhase()==PHASE_DAMAGE then pos=tc:GetBattlePosition() end
		if tc:IsLocation(LOCATION_MZONE) and tc:IsSetCard(0x31) and bit.band(pos,POS_FACEUP)~=0 then
			c20057949[tc:GetControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c20057949.clear(e,tp,eg,ep,ev,re,r,rp)
	c20057949[0]=false
	c20057949[1]=false
end
function c20057949.condition(e,tp,eg,ep,ev,re,r,rp)
	return c20057949[tp]
end
function c20057949.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetCountLimit(1)
	e1:SetLabel(Duel.GetTurnCount())
	e1:SetCondition(c20057949.spcon1)
	e1:SetOperation(c20057949.spop1)
	if Duel.GetTurnPlayer()==tp then
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
	else
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,1)
	end
	Duel.RegisterEffect(e1,tp)
end
function c20057949.spcon1(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnCount()~=e:GetLabel() and Duel.GetTurnPlayer()==tp
end
function c20057949.filter(c,e,tp)
	return c:IsSetCard(0x31) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c20057949.spop1(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c20057949.filter,tp,LOCATION_HAND,0,1,nil,e,tp)
		and Duel.SelectYesNo(tp,aux.Stringid(20057949,0)) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(20057949,1))
		e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
		e1:SetCountLimit(1)
		e1:SetTarget(c20057949.sptg2)
		e1:SetOperation(c20057949.spop2)
		e1:SetReset(RESET_PHASE+PHASE_STANDBY)
		Duel.RegisterEffect(e1,tp)
	end
end
function c20057949.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c20057949.spop2(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	if ft>2 then ft=2 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c20057949.filter,tp,LOCATION_HAND,0,1,ft,nil,e,tp)
	Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
end
