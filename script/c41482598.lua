--悪夢の蜃気楼
function c41482598.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c41482598.clear)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(41482598,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c41482598.drcon)
	e2:SetTarget(c41482598.drtg)
	e2:SetOperation(c41482598.drop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--discard
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(41482598,1))
	e3:SetCategory(CATEGORY_HANDES)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCondition(c41482598.dccon)
	e3:SetTarget(c41482598.dctg)
	e3:SetOperation(c41482598.dcop)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
end
function c41482598.clear(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
end
function c41482598.drcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)<4
end
function c41482598.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ht=Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,4-ht)
end
function c41482598.drop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local ht=Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)
	if ht<4 then
		Duel.Draw(tp,4-ht,REASON_EFFECT)
		e:GetLabelObject():SetLabel(4-ht)
	else e:GetLabelObject():SetLabel(0) end
end
function c41482598.dccon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and e:GetLabelObject():GetLabel()~=0
end
function c41482598.dctg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local de=e:GetLabelObject()
	e:SetLabel(de:GetLabel())
	de:SetLabel(0)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,e:GetLabel())
end
function c41482598.dcop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local sg=g:RandomSelect(tp,e:GetLabel())
	Duel.SendtoGrave(sg,REASON_EFFECT+REASON_DISCARD)
end
