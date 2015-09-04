--二重の落とし穴
function c67045174.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c67045174.condition)
	e1:SetTarget(c67045174.target)
	e1:SetOperation(c67045174.activate)
	c:RegisterEffect(e1)
	if not c67045174.global_check then
		c67045174.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_BATTLED)
		ge1:SetOperation(c67045174.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c67045174.checkop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local t=Duel.GetAttackTarget()
	if a and a:IsDualState() and a:IsStatus(STATUS_BATTLE_DESTROYED) then
		a:RegisterFlagEffect(67045174,RESET_PHASE+PHASE_DAMAGE,0,1)
	end
	if t and t:IsDualState() and t:IsStatus(STATUS_BATTLE_DESTROYED) then
		t:RegisterFlagEffect(67045174,RESET_PHASE+PHASE_DAMAGE,0,1)
	end
end
function c67045174.filter(c)
	return c:GetFlagEffect(67045174)~=0
end
function c67045174.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c67045174.filter,1,nil)
end
function c67045174.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil) end
	local sg=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,sg,sg:GetCount(),0,0)
end
function c67045174.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
	Duel.Destroy(sg,REASON_EFFECT)
end
