--異次元隔離マシーン
function c56460688.initial_effect(c)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c56460688.target)
	e1:SetOperation(c56460688.operation)
	c:RegisterEffect(e1)
	--return
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetOperation(c56460688.retop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c56460688.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToRemove,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(Card.IsAbleToRemove,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g1=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g2=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,0,LOCATION_MZONE,1,1,nil)
	g1:Merge(g2)
	if e:GetLabelObject() then
		e:GetLabelObject():DeleteGroup()
		e:SetLabelObject(nil)
	end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g1,2,0,0)
end
function c56460688.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if Duel.Remove(tg,0,REASON_EFFECT+REASON_TEMPORARY)~=0 then
		local g=Duel.GetOperatedGroup()
		local tc=g:GetFirst()
		while tc do
			tc:RegisterFlagEffect(56460688,RESET_EVENT+0x1fe0000,0,1)
			tc=g:GetNext()
		end
		c:RegisterFlagEffect(56460688,RESET_EVENT+0x17a0000,0,1)
		g:KeepAlive()
		e:SetLabelObject(g)
	end
end
function c56460688.retop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(56460688)==0 then return end
	if bit.band(r,REASON_DESTROY)~=0 then
		local g=e:GetLabelObject():GetLabelObject()
		local tc=g:GetFirst()
		while tc do
			if tc:GetFlagEffect(56460688)>0 then
				Duel.ReturnToField(tc)
			end
			tc=g:GetNext()
		end
		g:DeleteGroup()
		e:GetLabelObject():SetLabelObject(nil)
	end
end
