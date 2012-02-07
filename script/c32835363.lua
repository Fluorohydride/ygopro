--クラッキング
function c32835363.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c32835363.regop)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(32835363,0))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(32835363)
	e3:SetTarget(c32835363.damtg)
	e3:SetOperation(c32835363.damop)
	c:RegisterEffect(e3)
end
function c32835363.regop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(32835363)~=0 then return end
	local d1=false
	local d2=false
	local tc=eg:GetFirst()
	while tc do
		if tc:IsPreviousLocation(LOCATION_MZONE) and tc:IsType(TYPE_MONSTER) and bit.band(tc:GetReason(),0x41)==0x41 then
			if tc:GetOwner()==tp then d1=true
			else d2=true end
		end
		tc=eg:GetNext()
	end
	local evt_p=PLAYER_NONE
	if d1 and d2 then evt_p=PLAYER_ALL
	elseif d1 then evt_p=tp
	elseif d2 then evt_p=1-tp
	else return end
	Duel.RaiseSingleEvent(e:GetHandler(),32835363,e,0,0,evt_p,0)
	e:GetHandler():RegisterFlagEffect(32835363,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c32835363.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetTargetPlayer(ep)
	Duel.SetTargetParam(800)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,0,0,ep,800)
end
function c32835363.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		local d=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
		if ep==PLAYER_ALL then
			Duel.Damage(tp,d,REASON_EFFECT)
			Duel.Damage(1-tp,d,REASON_EFFECT)
		else
			Duel.Damage(ep,d,REASON_EFFECT)
		end
	end
end
