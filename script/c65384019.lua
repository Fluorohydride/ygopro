--アルケミー・サイクル
function c65384019.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c65384019.condition)
	e1:SetTarget(c65384019.target)
	e1:SetOperation(c65384019.activate)
	c:RegisterEffect(e1)
end
function c65384019.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c65384019.filter(c)
	return c:IsFaceup() and c:GetBaseAttack()~=0
end
function c65384019.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c65384019.filter,tp,LOCATION_MZONE,0,1,nil) end
end
function c65384019.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c65384019.filter,tp,LOCATION_MZONE,0,nil)
	local c=e:GetHandler()
	local fid=c:GetFieldID()
	local tc=g:GetFirst()
	while tc do
		if not tc:IsImmuneToEffect(e) then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_SET_BASE_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			e1:SetValue(0)
			tc:RegisterEffect(e1)
			tc:RegisterFlagEffect(65384019,RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_END,0,1,fid)
		end
		tc=g:GetNext()
	end
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(65384019,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EVENT_BATTLE_DESTROYED)
	e2:SetCondition(c65384019.drcon)
	e2:SetTarget(c65384019.drtg)
	e2:SetOperation(c65384019.drop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	e2:SetLabel(fid)
	Duel.RegisterEffect(e2,tp)
end
function c65384019.drfilter(c,fid)
	return c:GetFlagEffectLabel(65384019)==fid
end
function c65384019.drcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c65384019.drfilter,1,nil,e:GetLabel())
end
function c65384019.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c65384019.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
