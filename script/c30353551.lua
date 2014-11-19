--人海戦術
function c30353551.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_END_PHASE)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(30353551,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetCountLimit(1)
	e2:SetTarget(c30353551.target)
	e2:SetOperation(c30353551.operation)
	c:RegisterEffect(e2)
	if not c30353551.global_check then
		c30353551.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_BATTLED)
		ge1:SetOperation(c30353551.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c30353551.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c30353551.checkop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if a:IsStatus(STATUS_BATTLE_DESTROYED) and a:IsType(TYPE_NORMAL) and a:IsLevelBelow(2) then
		c30353551[a:GetControler()]=c30353551[a:GetControler()]+1
	end
	if d and d:IsStatus(STATUS_BATTLE_DESTROYED) and d:IsType(TYPE_NORMAL) and d:IsLevelBelow(2) then
		c30353551[d:GetControler()]=c30353551[d:GetControler()]+1
	end
end
function c30353551.clear(e,tp,eg,ep,ev,re,r,rp)
	c30353551[0]=0
	c30353551[1]=0
end
function c30353551.filter(c,e,tp)
	return c:IsType(TYPE_NORMAL) and c:IsLevelBelow(2) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c30353551.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c30353551[tp]~=0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,c30353551[tp],tp,LOCATION_DECK)
end
function c30353551.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c30353551.filter,tp,LOCATION_DECK,0,c30353551[tp],c30353551[tp],nil,e,tp)
	if g:GetCount()==0 then return end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	elseif ft>=g:GetCount() then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	else
		local sg=g:Select(tp,ft,ft,nil)
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		g:Sub(sg)
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
