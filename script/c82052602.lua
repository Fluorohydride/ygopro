--ガガガバック
function c82052602.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c82052602.condition)
	e1:SetTarget(c82052602.target)
	e1:SetOperation(c82052602.activate)
	c:RegisterEffect(e1)
	if not c82052602.global_check then
		c82052602.global_check=true
		c82052602[0]=false
		c82052602[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_BATTLE_DESTROYED)
		ge1:SetOperation(c82052602.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c82052602.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c82052602.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		local pos=tc:GetPosition()
		if tc:IsSetCard(0x54) and tc:IsLocation(LOCATION_GRAVE) and tc:IsReason(REASON_BATTLE)
			and tc:GetControler()==tc:GetPreviousControler() then
			c82052602[tc:GetControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c82052602.clear(e,tp,eg,ep,ev,re,r,rp)
	c82052602[0]=false
	c82052602[1]=false
end
function c82052602.filter(c,id,e,tp)
	return c:IsReason(REASON_BATTLE) and c:GetTurnID()==id and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c82052602.condition(e,tp,eg,ep,ev,re,r,rp)
	return c82052602[tp] and Duel.GetFlagEffect(tp,82052602)==0
end
function c82052602.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 
		and Duel.IsExistingMatchingCard(c82052602.filter,tp,LOCATION_GRAVE,0,1,nil,Duel.GetTurnCount(),e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
	Duel.RegisterFlagEffect(tp,82052602,RESET_PHASE+PHASE_END,0,1)
end
function c82052602.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft1=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft1==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c82052602.filter,tp,LOCATION_GRAVE,0,ft1,ft1,nil,Duel.GetTurnCount(),e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		Duel.BreakEffect()
		Duel.Damage(tp,g:GetCount()*600,REASON_EFFECT)
	end
end
