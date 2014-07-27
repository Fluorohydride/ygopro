--グリード
function c89405199.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--reg
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_DRAW)
	e2:SetCondition(c89405199.drcon)
	e2:SetOperation(c89405199.drop)
	c:RegisterEffect(e2)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(89405199,0))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetCountLimit(1)
	e3:SetCondition(c89405199.damcon)
	e3:SetTarget(c89405199.damtg)
	e3:SetOperation(c89405199.damop)
	c:RegisterEffect(e3)
end
function c89405199.drcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_EFFECT)~=0
end
function c89405199.drop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local flag=(ep==0 and 89405199 or 89405200)
	local ct=c:GetFlagEffectLabel(flag)
	if ct then
		c:SetFlagEffectLabel(flag,ct+ev)
	else
		c:RegisterFlagEffect(flag,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1,ev)
	end
end
function c89405199.damcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetFlagEffect(89405199)>0 or c:GetFlagEffect(89405200)>0
end
function c89405199.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local c=e:GetHandler()
	local ct1=c:GetFlagEffectLabel(89405199)
	local ct2=c:GetFlagEffectLabel(89405200)
	if ct1 and ct2 then
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,0)
	elseif ct1 then
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,0,ct1*500)
	elseif ct2 then
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1,ct2*500)
	end
end
function c89405199.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local ct1=c:GetFlagEffectLabel(89405199+tp)
	local ct2=c:GetFlagEffectLabel(89405199+1-tp)
	if ct1 then Duel.Damage(tp,ct1*500,REASON_EFFECT) end
	if ct2 then Duel.Damage(1-tp,ct2*500,REASON_EFFECT) end
end
