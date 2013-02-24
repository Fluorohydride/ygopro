--幻銃士
function c12958919.initial_effect(c)
	--token
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(12958919,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c12958919.sptg)
	e1:SetOperation(c12958919.spop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(12958919,1))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c12958919.damcon)
	e2:SetCost(c12958919.damcost)
	e2:SetTarget(c12958919.damtg)
	e2:SetOperation(c12958919.damop)
	c:RegisterEffect(e2)
	if not c12958919.global_check then
		c12958919.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_ATTACK_ANNOUNCE)
		ge1:SetOperation(c12958919.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c12958919.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:IsSetCard(0x49) then
		Duel.RegisterFlagEffect(tc:GetControler(),12958919,RESET_PHASE+PHASE_END,0,1)
	end
end
function c12958919.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,12958920,0x49,0x4011,500,500,4,RACE_FIEND,ATTRIBUTE_DARK) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c12958919.spop(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)
	if ft>ct then ft=ct end
	if ft<=0 then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,12958920,0x49,0x4011,500,500,4,RACE_FIEND,ATTRIBUTE_DARK) then return end
	local ctn=true
	while ft>0 and ctn do
		local token=Duel.CreateToken(tp,12958920)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
		ft=ft-1
		if ft<=0 or not Duel.SelectYesNo(tp,aux.Stringid(12958919,2)) then ctn=false end
	end
	Duel.SpecialSummonComplete()
end
function c12958919.damcon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c12958919.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,12958919)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetProperty(EFFECT_FLAG_OATH)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x49))
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
end
function c12958919.damfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x49)
end
function c12958919.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ct=Duel.GetMatchingGroupCount(c12958919.damfilter,tp,LOCATION_MZONE,0,nil)
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,ct*300)
end
function c12958919.damop(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local ct=Duel.GetMatchingGroupCount(c12958919.damfilter,tp,LOCATION_MZONE,0,nil)
	Duel.Damage(p,ct*300,REASON_EFFECT)
end
